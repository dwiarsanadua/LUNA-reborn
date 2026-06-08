--[[
  Quest 873: Adrian's Request 2
  Level Required: 92
  NPC Start: 90, NPC Complete: 534
  Prerequisites: [872]
  Rewards: EXP=5405129
]]

local fsm = require('fsm_engine')

local quest_873 = fsm:new({
    id = 873,
    name = "Adrian's Request 2",
    level_required = 92,
    npc_start = 90,
    npc_complete = 534,
    prerequisites = {872},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 873: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 534,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 873: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 873: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 873: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 534,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 873: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 873: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 873: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 534,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 873: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 873: Rewards given")
                self:give_rewards({
                    exp = 5405129,
                    items = {
                        { item_id = 21000271, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_873