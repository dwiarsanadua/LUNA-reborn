--[[
  Quest 195: [Scout Job Change] Path Less Traveled
  Level Required: 40
  NPC Start: 1, NPC Complete: 108
  Rewards: EXP=192000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_195 = fsm:new({
    id = 195,
    name = "[Scout Job Change] Path Less Traveled",
    level_required = 40,
    npc_start = 1,
    npc_complete = 108,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 195: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 195: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 195: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 108,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 195: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 195: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 108,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 195: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 195: Rewards given")
                self:give_rewards({
                    exp = 192000,
                    gold = 40000,
                    items = {
                        { item_id = 11007258, count = 1 },
                        { item_id = 11007258, count = 1 },
                        { item_id = 128, count = 10 },
                        { item_id = 11007257, count = 1 },
                        { item_id = 11007257, count = 1 },
                        { item_id = 129, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_195