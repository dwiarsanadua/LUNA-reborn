--[[
  Quest 275: [Emblem of Exploration] Great Unknown
  Level Required: 70
  NPC Start: 7, NPC Complete: 54
  Rewards: EXP=1491000, Gold=50000
]]

local fsm = require('fsm_engine')

local quest_275 = fsm:new({
    id = 275,
    name = "[Emblem of Exploration] Great Unknown",
    level_required = 70,
    npc_start = 7,
    npc_complete = 54,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 275: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 275: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 275: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 49,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 33,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 4,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 6,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 32,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 96,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 275: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 275: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 54,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 275: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 275: Rewards given")
                self:give_rewards({
                    exp = 1491000,
                    gold = 50000,
                    items = {
                        { item_id = 178, count = 1 },
                        { item_id = 178, count = 1 },
                        { item_id = 178, count = 1 },
                        { item_id = 178, count = 1 },
                        { item_id = 178, count = 1 },
                        { item_id = 178, count = 1 },
                        { item_id = 179, count = 1 },
                        { item_id = 179, count = 1 },
                        { item_id = 179, count = 1 },
                        { item_id = 179, count = 1 },
                        { item_id = 179, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 180, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 181, count = 1 },
                        { item_id = 178, count = 5 },
                        { item_id = 179, count = 5 },
                        { item_id = 180, count = 5 },
                        { item_id = 181, count = 5 },
                        { item_id = 182, count = 1 },
                        { item_id = 182, count = 1 },
                        { item_id = 30000481, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_275