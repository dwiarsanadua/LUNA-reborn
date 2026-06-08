--[[
  Quest 276: [Emblem of Faith] Temple Relics
  Level Required: 70
  NPC Start: 9, NPC Complete: 55
  Rewards: EXP=1491000, Gold=50000
]]

local fsm = require('fsm_engine')

local quest_276 = fsm:new({
    id = 276,
    name = "[Emblem of Faith] Temple Relics",
    level_required = 70,
    npc_start = 9,
    npc_complete = 55,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 276: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 276: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 276: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 276: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 276: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 276: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 276: Rewards given")
                self:give_rewards({
                    exp = 1491000,
                    gold = 50000,
                    items = {
                        { item_id = 30000506, count = 1 },
                        { item_id = 30000507, count = 1 },
                        { item_id = 30000505, count = 1 },
                        { item_id = 183, count = 15 },
                        { item_id = 184, count = 1 },
                        { item_id = 30000482, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_276