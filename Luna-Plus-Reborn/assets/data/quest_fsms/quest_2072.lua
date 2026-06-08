--[[
  Quest 2072: [Daily] The Captains Ressurection (Advanced)
  Level Required: 50
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=832138
]]

local fsm = require('fsm_engine')

local quest_2072 = fsm:new({
    id = 2072,
    name = "[Daily] The Captains Ressurection (Advanced)",
    level_required = 50,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2072: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2072: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2072: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 704,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2072: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 705,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2072: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 706,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2072: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 707,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2072: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2072: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2072: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2072: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2072: Rewards given")
                self:give_rewards({
                    exp = 832138,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2072