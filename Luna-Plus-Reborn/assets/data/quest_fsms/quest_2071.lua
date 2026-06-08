--[[
  Quest 2071: [Daily] The Captains Ressurection (Intermediate)
  Level Required: 50
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=364970
]]

local fsm = require('fsm_engine')

local quest_2071 = fsm:new({
    id = 2071,
    name = "[Daily] The Captains Ressurection (Intermediate)",
    level_required = 50,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2071: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2071: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2071: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 700,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2071: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 701,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2071: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 702,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2071: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 703,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2071: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2071: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2071: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2071: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2071: Rewards given")
                self:give_rewards({
                    exp = 364970,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2071