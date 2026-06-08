--[[
  Quest 851: [Party]Monsters of the wreck 2
  Level Required: 87
  NPC Start: 502, NPC Complete: 502
  Rewards: EXP=3799344
]]

local fsm = require('fsm_engine')

local quest_851 = fsm:new({
    id = 851,
    name = "[Party]Monsters of the wreck 2",
    level_required = 87,
    npc_start = 502,
    npc_complete = 502,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 851: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 851: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 851: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 684,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 851: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 686,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 851: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 693,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 851: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 695,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 851: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 851: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 851: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 502,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 851: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 851: Rewards given")
                self:give_rewards({
                    exp = 3799344,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_851