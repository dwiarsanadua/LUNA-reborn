--[[
  Quest 797: [Party] In the Cave of Predators
  Level Required: 66
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=804591, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_797 = fsm:new({
    id = 797,
    name = "[Party] In the Cave of Predators",
    level_required = 66,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 797: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 797: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 797: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 797: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 797: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 797: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 797: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 797: Rewards given")
                self:give_rewards({
                    exp = 804591,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_797