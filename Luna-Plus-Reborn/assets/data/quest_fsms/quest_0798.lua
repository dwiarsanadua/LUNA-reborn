--[[
  Quest 798: [Party] In the Sinking Cave
  Level Required: 66
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=804591, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_798 = fsm:new({
    id = 798,
    name = "[Party] In the Sinking Cave",
    level_required = 66,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 798: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 798: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 798: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 102,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 798: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 798: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 798: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 798: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 798: Rewards given")
                self:give_rewards({
                    exp = 804591,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_798