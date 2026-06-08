--[[
  Quest 834: Suppress Gwonjok 2
  Level Required: 79
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [833]
  Rewards: EXP=1996587, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_834 = fsm:new({
    id = 834,
    name = "Suppress Gwonjok 2",
    level_required = 79,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {833},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 834: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 834: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 834: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 74,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 834: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 834: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 834: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 834: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 834: Rewards given")
                self:give_rewards({
                    exp = 1996587,
                    gold = 41009,
                })
            end,
            transitions = {},
        },
    },
})

return quest_834