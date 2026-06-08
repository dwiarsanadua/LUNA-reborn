--[[
  Quest 601: Experiments in the Secret Dungeon
  Level Required: 150
  NPC Start: 135, NPC Complete: 315
]]

local fsm = require('fsm_engine')

local quest_601 = fsm:new({
    id = 601,
    name = "Experiments in the Secret Dungeon",
    level_required = 150,
    npc_start = 135,
    npc_complete = 315,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 601: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 135,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 601: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 601: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 603,
                    count = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 604,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 605,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 606,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 607,
                    count = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 135,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 310,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 311,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 312,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 313,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 314,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 315,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 601: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 601: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 315,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 601: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 601: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001193, count = 1 },
                        { item_id = 30000745, count = 1 },
                        { item_id = 30000746, count = 1 },
                        { item_id = 30000747, count = 1 },
                        { item_id = 30000748, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_601