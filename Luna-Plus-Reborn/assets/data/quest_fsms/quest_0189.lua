--[[
  Quest 189: [Swordsman Job Change] Mind and Body
  Level Required: 40
  NPC Start: 10, NPC Complete: 103
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_189 = fsm:new({
    id = 189,
    name = "[Swordsman Job Change] Mind and Body",
    level_required = 40,
    npc_start = 10,
    npc_complete = 103,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 189: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 189: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 189: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 103,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 189: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 189: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 103,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 189: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 189: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 102, count = 10 },
                        { item_id = 103, count = 1 },
                        { item_id = 104, count = 1 },
                        { item_id = 105, count = 1 },
                        { item_id = 106, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_189