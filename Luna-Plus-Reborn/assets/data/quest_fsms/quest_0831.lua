--[[
  Quest 831: Kierra Conspiracy
  Level Required: 78
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [412]
  Rewards: EXP=1850968, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_831 = fsm:new({
    id = 831,
    name = "Kierra Conspiracy",
    level_required = 78,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {412},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 831: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 831: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 831: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 71,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 831: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 831: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 831: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 831: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 831: Rewards given")
                self:give_rewards({
                    exp = 1850968,
                    gold = 41009,
                })
            end,
            transitions = {},
        },
    },
})

return quest_831