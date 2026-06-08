--[[
  Quest 2078: [Daily] The Spector of the Article(Advanced)
  Level Required: 90
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=12691894
]]

local fsm = require('fsm_engine')

local quest_2078 = fsm:new({
    id = 2078,
    name = "[Daily] The Spector of the Article(Advanced)",
    level_required = 90,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2078: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2078: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2078: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 671,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2078: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 672,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2078: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 673,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2078: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 674,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2078: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2078: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2078: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2078: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2078: Rewards given")
                self:give_rewards({
                    exp = 12691894,
                    items = {
                        { item_id = 30000836, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2078