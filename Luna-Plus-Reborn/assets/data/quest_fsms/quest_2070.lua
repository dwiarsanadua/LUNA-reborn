--[[
  Quest 2070: [Daily] The Captains Ressurection (Beginner)
  Level Required: 50
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=158549
]]

local fsm = require('fsm_engine')

local quest_2070 = fsm:new({
    id = 2070,
    name = "[Daily] The Captains Ressurection (Beginner)",
    level_required = 50,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2070: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2070: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2070: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 696,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2070: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 697,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2070: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 698,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2070: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 699,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2070: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2070: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2070: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2070: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2070: Rewards given")
                self:give_rewards({
                    exp = 158549,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2070