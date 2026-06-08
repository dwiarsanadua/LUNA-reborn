--[[
  Quest 2076: [Daily] The Spector of the Article(Beginner)
  Level Required: 90
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=3435786
]]

local fsm = require('fsm_engine')

local quest_2076 = fsm:new({
    id = 2076,
    name = "[Daily] The Spector of the Article(Beginner)",
    level_required = 90,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2076: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2076: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2076: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 663,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2076: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 664,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2076: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 665,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2076: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 666,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2076: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2076: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2076: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2076: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2076: Rewards given")
                self:give_rewards({
                    exp = 3435786,
                    items = {
                        { item_id = 30000836, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2076