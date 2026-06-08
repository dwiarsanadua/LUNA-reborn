--[[
  Quest 274: [Emblem of Sacrifice] Helping Hand
  Level Required: 70
  NPC Start: 6, NPC Complete: 36
  Rewards: EXP=1491000, Gold=50000
]]

local fsm = require('fsm_engine')

local quest_274 = fsm:new({
    id = 274,
    name = "[Emblem of Sacrifice] Helping Hand",
    level_required = 70,
    npc_start = 6,
    npc_complete = 36,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 274: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 274: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 274: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 31,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 274: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 274: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 274: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 274: Rewards given")
                self:give_rewards({
                    exp = 1491000,
                    gold = 50000,
                    items = {
                        { item_id = 176, count = 1 },
                        { item_id = 174, count = 1 },
                        { item_id = 175, count = 1 },
                        { item_id = 177, count = 1 },
                        { item_id = 30000480, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_274