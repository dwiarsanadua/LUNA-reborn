--[[
  Quest 284: [Emblem of Honor] Crash Cleanup
  Level Required: 70
  NPC Start: 19, NPC Complete: 77
  Prerequisites: [283]
  Rewards: EXP=1680000, Gold=50000
]]

local fsm = require('fsm_engine')

local quest_284 = fsm:new({
    id = 284,
    name = "[Emblem of Honor] Crash Cleanup",
    level_required = 70,
    npc_start = 19,
    npc_complete = 77,
    prerequisites = {283},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 284: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 284: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 284: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 284: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 284: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 73,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 284: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 76,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 284: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 77,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 284: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 284: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 284: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 77,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 284: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 284: Rewards given")
                self:give_rewards({
                    exp = 1680000,
                    gold = 50000,
                    items = {
                        { item_id = 196, count = 1 },
                        { item_id = 197, count = 1 },
                        { item_id = 198, count = 1 },
                        { item_id = 196, count = 1 },
                        { item_id = 197, count = 1 },
                        { item_id = 198, count = 1 },
                        { item_id = 30000485, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_284